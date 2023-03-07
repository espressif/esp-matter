# Releases

Releases are tied to a specific revision of the Zigbee Cluster Library (ZCL). These files are released from a git repo which is tagged based on the release version of the ZCL. At the time of the initial conversion, the ZCL document was at release 7. Multiple releases of the XML files may be made against a single ZCL version. the tagging mechanism is designed to handle this and follows the format zcl<ZCL Version>.<XML Release Version> where ZCL Version is the version of the ZCL document and the XML Release Version is a counter based on the source document which starts from 0. This means the initial release tag for ZCLr7 is zcl7.0. Subsequent releases of XML against ZCLr7 will be zcl7.1, zcl7.2, etc. When a new ZCL, such as ZCLr8 is released, the tags will be come zcl8.0, zcl8.1, etc.

# Eclipse Setup

To configure eclipse to properly work with the XML files you'll need a version of eclipse that supports the XML Editors and Tools. You can [compare different versions](https://www.eclipse.org/downloads/packages/compare). Once installed, follow the instructions below to get set up for validation. These assume you've already got a local copy of the GIT repo checked out.

1. Preferences > XML > XML Files > Validation
2. Check "Process XML Inclusions"
3. Check "Honour all XML schema locations"
4. Click "Apply and Close"
5. File > Import
6. General > File System
7. Select the GIT repo
8. Import
9. Open "library.xml"
10. Right click, "Validate"

# Converting Clusters

There should be a base file for each cluster available in the project, produced by scraping the word documents for the ZCL. These files need to be reviewed by a human and edited as necessary. The process to do this at a high level is as follows

1. Create a branch for your work (use your name)
2. Start to edit the files
3. Commit when you're done and they validate
4. Make a pull request

## library.xml

Each file should be referenced in library.xml as it is reviewd. At the bottom of the file you'll see some statements like the following.

```xml
	<xi:include href="global.xml" parse="xml" />
	<xi:include href="Basic.xml" parse="xml" />
```

Duplicate the line for Basic.xml for your cluster, and insert it into the correct position based on the ordering in the ZCL document.

## Clusters

Each base cluster is defined with the following basic structure

```xml
<zcl:cluster xmlns:zcl="http://zigbee.org/zcl/clusters"
             xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
             xmlns:type="http://zigbee.org/zcl/types"
             xmlns:xi="http://www.w3.org/2001/XInclude"
             xmlns:schemaLocation="http://zigbee.org/zcl/clusters cluster.xsd http://zigbee.org/zcl/types type.xsd"
             id="0000" revision="0" name="Basic">
  <classification role="utility" picsCode="B" />
  <type:type />
  <server>...</server>
  <client>...</client>
  <tags>...</tags>
</zcl:cluster>
```

The classification element is required, all of the others can be omitted if there is no content. The classification information is given in the ZCL document and must include the role and picsCode. It may also include the primaryTransaction if the role is application.

The type:type definition allows for the definition of complex types, or types which are used by multiple attributes and/or commands. This must be used whenever an enumeration, bitmap, etc. is reused. For more information see the Types section below.

The server and client define the attributes and commands that comprise the respective server and client sides of a cluster. For more information see the Clusters section below.

The tags section allows for the definition of tags at the cluster level which are then referenced by the commands which the tag may be used to extend. For more information see the Tags section below.

## Cluster Inheritance

Clusters may inherit from a base cluster. Each derived cluster is defined with the following basic structure

```xml
<zcl:derivedCluster xmlns:zcl="http://zigbee.org/zcl/clusters"
             xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
             xmlns:type="http://zigbee.org/zcl/types"
             xmlns:xi="http://www.w3.org/2001/XInclude"
             xmlns:schemaLocation="http://zigbee.org/zcl/clusters cluster.xsd http://zigbee.org/zcl/types type.xsd"
             id="001c" revision="0" name="PulseWidthModulation" inheritsFrom="Level">
  <classification picsCode="PWM" />
  <server>...</server>
  <client>...</client>
</zcl:derivedCluster>
```

The classification element is required, all of the others can be omitted if there is no content. The classification information is given in the ZCL document and must include the picsCode.

The server and client allow the attributes and commands that are modified by the respective server and client sides of a cluster to be expressed. For more information see the Derived Clusters section below.

## Naming

All names in the XML must adhere to regular expression `[A-Za-z0-9\-]*`. This means that in general, names should be such that the first letter of each work is upper-case, and the rest of the word lower-case. There should also be no spaces in the name.

# Types

Types can be defined in the XML. There is the opportunity to create both base types and derived types. Then restrictions may be used to properly form the type. The minimum required set of fields for an attribute definition are id, name, and short. The discrete attribute is optional, and if omitted defaults to false which indicates the data type is considered to be analog. Finally, an inheritsFrom attribute indicates the short value for the attribute from which the attribute derives.

Examples can be seen in the library.xml which defines the base data types and some subclassed types such as the zclStatus type. More advanced mechanisms such as multi-element fields are visible in the global.xml which defines multi-element types which are used for repetition in messages like the Read Attributes Response.

## Bitmaps

Bitmap types can be expanded to subfields in the schema using the bitmap tag, which allows a set of fields to be specified using a mask and shift. A bitmap is defined as follows:

```xml
<attribute id="0014" name="DisableLocalConfig" type="map8" writable="true" default="0">
  <bitmap>
    <element name="DisableResetToFactoryDefaults" type="bool" mask="01" />
    <element name="DisableDeviceConfiguration" type="bool" mask="02" shiftRight="1" />
    <element name="MyEnum" type="enum8" mask="0C" shiftRight="2">
      <restriction>
        <type:enumeration name="FirstValue" value="00" />
        <type:enumeration name="SecondValue" value="01" />
        <type:enumeration name="ThirdValue" value="02" />
        <type:enumeration name="FourthValue" value="03" />
      </restriction>
    </element>
  </bitmap>
</attribute>
```

As an example, the following bitmap `0000 1010` would be broken down as follows:

- DisableResetToFactoryDefaults: (0000 1010 & 0000 0001) >> 0 = 0000 0000 >> 0 = 0000 0000 = 0 (False)
- DisableDeviceConfiguration: (0000 1010 & 0000 0010) >> 1 = 0000 0010 >> 1 = 0000 0001 = 1 (True)
- MyEnum: (0000 1010 & 0000 1100) >> 2 = 0000 1000 >> 2 = 0000 0010 = 2 (ThirdValue)

Each element specified may have the restrictions listed below.

## Restrictions

There are a number of restrictions available for types. These build the basis of the typing system and are as follows

- enumeration - see the Enumerations section below
- enumeratedRange - see the Enumerations section below
- minExclusive - sets a minimum that doesn't include the value specified, i.e. a field of this type must be strictly greater than the value
- minExclusiveRef - sets a minimum that is based on the value of the referenced attribute. The value of the referenced attribute is excluded from the range
- minExclusiveExpression - sets a minimum that is based on an expression. The value returned by the expression is excluded from the range. This construct may only be used if no other mechanism will meet the needs.
- minInclusive - sets a minimum that includes the value specified, i.e. a field of this type must be greater than or equal than the value
- minInclusiveRef - sets a minimum that is based on the value of the referenced attribute. The value of the referenced attribute is included in the range
- minInclusiveExpression - sets a minimum that is based on an expression. The value returned by the expression is included from the range. This construct may only be used if no other mechanism will meet the needs.
- maxExclusive - sets a maximum that doesn't include the value specified, i.e. a field of this type must be strictly less than the value
- maxExclusiveRef - sets a maximum that is based on the value of the referenced attribute. The value of the referenced attribute is excluded from the range
- maxExclusiveExpression - sets a maximum that is based on an expression. The value returned by the expression is excluded from the range. This construct may only be used if no other mechanism will meet the needs.
- maxInclusive - sets a maximum that includes the value specified, i.e. a field of this type must be less than or equal to the value
- maxInclusiveRef - sets a maximum that is based on the value of the referenced attribute.
- maxInclusiveExpression - sets a maximum that is based on an expression. The value returned by the expression is included from the range. This construct may only be used if no other mechanism will meet the needs.
- complexConstraint - specifies a logical expression that must evaluate to true for the attribute value or command field value to be considered valid. Used to describe more complex interdependencies than cannot be expressed by other restriction types. This construct may only be used if no other mechanism will meet the needs.
- totalDigits - see http://www.w3.org/TR/xmlschema11-2/#element-totalDigits
- fractionDigits - see http://www.w3.org/TR/xmlschema11-2/#element-fractionDigits
- length - specifies an exact length, generally used for a string.
- minLength - specifies the minimum length that a type must take, generally used for a string or a list/array
- maxLength - specifies the maximum length that a type must take, generally used for a string or a list/array
- pattern - specifies a regular expression pattern which a string must match
- sequence - specifies a sequence of fields, should only be used for restrictions on command fields
- special - specifies a special value, see the Special Values section below
- invalid - species an invalid value as defined in the Zigbee specification

## Enumerations

Enumerations are defined as a value and a short name. The short name is intended to be such that it could be used as a constant name in code. Verbose descriptions are not permitted, and instead a descriptive name should be used. The value is specified as a HexBinary, which means that it's specified in Hex, and must be expressed as a multiple of two hexadecimal characters.

In some cases, the spec defines a range of enumeration values for use by a manufacturer. Sometimes this is a significant range which cannot easily be represented by making multiple enumeration statements. In this case, the enumeratedRange restriction can be used to specify a min and max value of the range along with a name.

## Special Values

In some cases, a special value is defined by the Zigbee specification. In these cases, the special value along with a descriptor should be defined using this tag. Special values take precedence over other restrictions imposed (e.g. a special value may fall outside the min/max range for the attribute).

## Invalid Values

For some data types of fields, a specific value is defined to indicate the data contained is invalid / unused / absent / ignored.

## anyType

The anyType type is defined in the global commands XML as this construct only applies to Zigbee pro. It must never be used in a cluster specific command.

The type is defined as two field, the first is the Type, which is required to be a type name. The second is the Value which is defiend as type unk as the type is unknown without the information provided by Type. When interpreting data based on this type, it should be done using the details of the type named in the Type field.

## ReportableChange

The ReportableChange field is specified as unk, but must be interpreted based on the value of AttributeType in each of the commands in which the field is included.

# Clusters

Each cluster is defined as a client / server pair. Inside each side of these pairs exists a set of attributes and/or commands. Each side is required to implement several global attributes. There may be additional attributes and commands defined. See the Attributes and Commands sections below for more information.

A cluster also has some additional information such as id, name, and how it is classified.

| Attribute | Required | Description                                                  |
| --------- | -------- | ------------------------------------------------------------ |
| id        | true     | The Zigbee cluster id, as a HexBinary (ie, 4 hex characters) |
| name      | true     | The name of the cluster, as per the Naming section above     |
| revision  | true     | The cluster revision described by this document              |

## Classification

The classification tag contains information about the role the cluster takes and any hierarchy information.

| Attribute          | Required | Description                                                           |
| ------------------ | -------- | --------------------------------------------------------------------- |
| role               | true     | The role that the cluster takes (application or utility).             |
| picsCode           | true     | The code which is used for PICS items relating to this cluster        |
| primaryTransaction | false    | For application clusters, the primary transaction type. Defaults to 1 |

## Attributes

Attribute definitions look like the following.

```xml
<attributes>
  <attribute id="0000" name="ZCLVersion" type="uint8" required="true" max="255" default="3" />
  <attribute id="0008" name="GenericDevice-Class" type="enum8" default="255">
    <restriction>
      <type:enumeration value="00" name="Lighting" />
    </restriction>
  </attribute>
</attributes>
```

An attribute may be specified using the following attributes in the XML.

| Attribute      | Required | Description                                                                                                                                                                                          |
| -------------- | -------- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| id             | true     | The Zigbee attribute id, as a HexBinary (ie, 4 hex characters)                                                                                                                                       |
| name           | true     | The name of the attribute, as per the Naming section above                                                                                                                                           |
| type           | true     | The short name of the attribute type                                                                                                                                                                 |
| readable       | false    | If the attribute is readable OTA. Defaults to true                                                                                                                                                   |
| writable       | false    | If the attribute is writable OTA. Defaults to false.                                                                                                                                                 |
| writeOptional  | false    | If the attribute is specified as writable this indicates if the write is required. Defaults to false.                                                                                                |
| writableIf     | false    | Indicates an expression that specifies the writability of the attribute. Defaults to true. Note: An attribute is only writable if this attribute and the writable attribute are true.                |
| reportRequired | false    | If attribute is required to be reportable. Defaults to false                                                                                                                                         |
| sceneRequired  | false    | If attribute is required to be part of the scene extensions. Defaults to false                                                                                                                       |
| required       | false    | If the attribute is mandatory. Defaults to false                                                                                                                                                     |
| requiredIf     | false    | Allows for an expression to be implemented which indicates the conditions in which an attribute is mandatory. Defaults to false                                                                      |
| min            | false    | A decimal integer specifying the min value (inclusive). Not for use on strings, use minLength restriction instead. Defaults to 0                                                                     |
| max            | false    | A decimal integer specifiying the max value (inclusive). Not for use on strings, use maxLength restriction instead. No Default.                                                                      |
| default        | false    | Specifies the default value of an attribute. No Default                                                                                                                                              |
| defaultRef     | false    | Specifies that the default value of the attribute takes the value of the referenced attribute. Must be another attriibute in this cluster. Referenced by name, schema forces this during validation. |
| deprecated     | false    | Indicates that an attribute has been deprecated                                                                                                                                                      |

Inside an attribute definition, either a bitmap or a series of restrictions may be specified. An example of restrictions can be seen in the example attribute definition for `GenericDevice-Class`. For an example of bitmap definitions, see the Bitmaps section.

## Commands

Command definitions look like the following.

```xml
<commands>
  <command id="00" name="AddGroup" required="true">
    <fields>
      <field name="GroupId" type="uint16" />
      <field name="GroupName" type="string" />
    </fields>
    <tag ref="AddressAssignmentMode" />
    <tag ref="MulticastIPv6Address" />
    <tag ref="GroupPort" />
  </command>
  <command id="04" name="RemoveAllGroups" required="true" />
</commands>
```

A command may be specified using the following attributes in XML

| Attribute  | Required | Description                                                                                                                  |
| ---------- | -------- | ---------------------------------------------------------------------------------------------------------------------------- |
| id         | true     | The Zigbee command id, as a HexBinary (ie, 2 hex characters)                                                                 |
| name       | true     | The name of the command, as per the Naming section above                                                                     |
| required   | false    | If the command is mandatory. Defaults to false                                                                               |
| requiredIf | false    | Allows for an expression to be implemented which indicates the conditions in which a command is mandatory. Defaults to false |
| deprecated | false    | Indicates that a command has been deprecated                                                                                 |

Inside a command definition, a series of fields followed by tag definitions is permitted.

Each field is defined with the following attributes in XML

| Attribute        | Required | Description                                                                                                                                                                       |
| ---------------- | -------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| name             | true     | The name of the command, as per the Naming section above                                                                                                                          |
| type             | true     | The short name of the field type                                                                                                                                                  |
| array            | false    | If the field is an array. Defaults to false                                                                                                                                       |
| arrayLengthSize  | false    | When an array is present, specifies the size (in octets) of the field that specifies the array length. Defaults to 1.                                                             |
| arrayLengthField | false    | When the number of elements in an array field is specified by another field which does not immediately precede an array field, that field may be referenced using this attribute. |
| presentIf        | false    | Specifies an expression (as described in the Expressions section) that indicates if the field is present. Defaults to true, i.e. Field is present.                                |
| requiredIf       | false    | Specifies an expression (as described in the Expressions section) that indicates if the field is required. Defaults to false, i.e. the command is not mandatory                   |
| deprecated       | false    | Indicates that a command field has been deprecated                                                                                                                                |

Similar to an attribute, a field may contain definitions of bitmaps or restrictions.

# Derived Clusters

Each derived cluster is defined as a client / server pair. Inside each side of these pairs exists modifiers to the base cluster set of attributes and/or commands. Additional attributes and commands may not be defined, but must be added to the base cluster. See the Derived Attributes and Derived Commands sections below for more information.

A cluster also has some additional information such as id, name, and how it is classified.

| Attribute    | Required | Description                                                        |
| ------------ | -------- | ------------------------------------------------------------------ |
| id           | true     | The Zigbee cluster id, as a HexBinary (ie, 4 hex characters)       |
| name         | true     | The name of the cluster, as per the Naming section above           |
| revision     | true     | The cluster revision described by this document                    |
| inheritsFrom | true     | Specifies the name of the cluster from which this cluster inherits |

## Classification

The classification tag contains information about the role the cluster takes and any hierarchy information.

| Attribute          | Required | Description                                                           |
| ------------------ | -------- | --------------------------------------------------------------------- |
| role               | true     | The role that the cluster takes (application or utility).             |
| picsCode           | true     | The code which is used for PICS items relating to this cluster        |
| primaryTransaction | false    | For application clusters, the primary transaction type. Defaults to 1 |

## Derived Attributes

Derived attribute modifications look like the following.

```xml
<attributes>
  <attribute ref="GenericDevice-Class" required="true" />
</attributes>
```

An attribute may be specified using the following attributes in the XML.

| Attribute      | Required | Description                                                                                                                     |
| -------------- | -------- | ------------------------------------------------------------------------------------------------------------------------------- |
| ref            | true     | The name of the attribute in the base cluster which is being modified, as per the Naming section above                          |
| reportRequired | false    | If attribute is required to be reportable. Defaults to false                                                                    |
| sceneRequired  | false    | If attribute is required to be part of the scene extensions. Defaults to false                                                  |
| required       | false    | If the attribute is mandatory. Defaults to false                                                                                |
| requiredIf     | false    | Allows for an expression to be implemented which indicates the conditions in which an attribute is mandatory. Defaults to false |

## Derived Commands

Derived commands definitions look like the following.

```xml
<commands>
  <command ref="AddGroup" required="true">
    <tag ref="AddressAssignmentMode" />
    <tag ref="MulticastIPv6Address" />
    <tag ref="GroupPort" />
  </command>
</commands>
```

A derived command may modify a command in the cluster using the following attributes in XML

| Attribute  | Required | Description                                                                                                                                                     |
| ---------- | -------- | --------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| ref        | true     | The name of the command in the base cluster which is being modified, as per the Naming section above                                                            |
| required   | false    | If the command is mandatory. Defaults to the value specified for the command in the base cluster                                                                |
| requiredIf | false    | Allows for an expression to be implemented which indicates the conditions in which a command is mandatory. Defaults to the value specified in the base cluster. |

A derived command may specify additional tags which may be included. It may also specify that they are required (by default tags are optional).

# Arrays

Conversion of an array in a command field is done using the following process.

1.  Identify the type of each element in the array. It may be necessary to create a record-like type which contains other types using the sequence restriction. An example of this can be seen in:

        <type:type id="ff" short="readAttributeResponseRecord" name="Read Attributes Status Record">
            <restriction>
        	    <type:sequence>
        		    <field name="AttributeIdentifier" type="attribId" />
        		    <field name="Status" type="zclStatus" />
        		    <field name="Atttribute" type="anyType" presentIf="Status=0" />
        	    </type:sequence>
            </restriction>
        </type:type>

         <command id="01" name="ReadAttributesResponse" required="true">
             <fields>
                 <field name="ReadAttributeRecord" type="readAttributeResponseRecord" array="true" arrayLengthSize="0" />
             </fields>
         </command>

2.  Identify how the number of elements in the array is determined. This could be through a count field that immediately precedes the elements, a count field that exists elsewhere in the command, or implicitly (i.e., the rest of the frame is consumed by array elements)

3.  If the array elements are immediately preceded by a count field (which is not a component of a bitmap), then the array should be defined by an entry for the elements with array="true". If the size counter was not 8 bits, then the arrayLengthSize should also be set to the number of octets. For example, an array of 8 bit unsigned integers with a 16-bit length would be defined as follows:

        <field name="MyArray" type="uint8" array="true" arrayLengthSize="2" />

4.  If the field that specifies the number of elements in the array is elsewhere in the command, it must be specified properly as a numeric type and then referenced by the array field in the XML. The element may be part of a bitmap or a separate field.

        <field name="MyBitmap" type="map8">
             <bitmap>
                 <element name="TransitionCount" type="uint8" mask="0f" />
                 ...
             </bitmap>
        </field>
        <field name="Transitions" type="TransitionType" array="true" arrayLengthField="MyBitmap.NumberOfTransitions" />

5.  If there is no field that specifies the number of entries, then this means that they must consume the rest of the message. This is indicated by setting the arrayLengthSize to 0.

        <field name="MyArray" type="uint8" array="true" arrayLengthSize="0" />

# Expressions

Logical expressions in the XML SHALL be expressed using the operators specified in [XPath 1.0](https://www.w3.org/TR/1999/REC-xpath-19991116/#section-Expressions). Specifically, section 3. This provides the operators or, and, =, !=, <=, <, >=, >. References to other fields in a command are done through name. Sub-elements in a bitmap may also be referenced using dot notation (ie, field.bitmapItem).

In addition, the following functions are available for expressions

| Function | Description |
| implements(X) | Returns true when a specific implementation implements the attribute or command named by X. Uses the name, not the id |
| min(X, Y, ...) | Returns the minimum value of the provided arguments. May take numeric literals or attribute names as references. |
| max(X, Y, ...) | Returns the maximum value of the provided arguments. May take numeric literals or attribute names as references. |
| revision() | Returns the revision of the cluster for which an attribute is implemented. In the global context, this is interpreted as the value of the specific cluster into which the global attribute or command has been inserted. |

# Patterns

## Sets of dependent attributes / commands

In some clusters, there are a number of optional attributes or commands that are specified as having to be implemented together when implemented. The procedure used to capture this in XML is as follows.

1. Identify all attributes and commands that are grouped together
2. Select one attribute as the key attribute or command
3. Add a requiredIf statement to the key attribute which uses the implements() function for all the other attributes and commands which were identified in the first step. These are added using the or keyword
   <attribute id="0000" name="Attribute1" ... requiredIf="implements(Attribute2) or implements(Attribute3)" />
4. For each remaining attribute and command (ie, not the key attribute or command), add a requiredIf pointing to the key attribute or command using the implements() function.

   <attribute id="0001" name="Attribute2" ... requiredIf="implements(Attribute1)" />

## Fields in a Command

Below is an example of optional fields in a command using the presentIf capabilities. It's implemented as a type because it's required to be repeated, etc.

```xml
<type:type id="ff" short="configureReportingRecord" name="Attribute Reporting Configuration Record">
  <restriction>
    <type:sequence>
      <field name="Direction" type="reportingRole" />
      <field name="AttributeIdentifier" type="attribId" />
      <field name="AttributeType" type="zclType" presentIf="Direction = 0" />
      <field name="MinimumReportingInterval" type="uint16" presentIf="Direction = 0" />
      <field name="MaximumReportingInterval" type="uint16" presentIf="Direction = 0" />
      <field name="ReportableChange" type="unk" presentIf="Direction = 0" />
      <field name="Timeout" type="uint16" presentIf="Direction = 1" />
    </type:sequence>
  </restriction>
</type:type>
```

# Tags

Tags allow for the extension of an existing command. A tag is defined as an identifier, a type and a name. It can then be added to some or all of the commands that exist in the cluster. Tags are created in the tags section which follows the cluster side definitions. A command may then reference the tags that may be used with it. An example may be seen in the groups cluster, specifically the AddGroup command.

# Viewing As HTML

The library.xml field has a stylesheet associated which can generate a HTML representation of the ZCL Library. This can be accessed by running the following python command inside the directory. The <port> attribute indicates the port you want to run on.

python -m SimpleHTTPServer <port>

Once you've started the server, you can navigate to http://localhost/library.xml to view the library. Note that if you specify a port, you will need to modify this URI.
