# Access control features

ZAP supports access control on all ZCL entities. It's down to the implementation of the SDK to map
these features to the required and supported access control SDK features. ZAP generally just provides a data model and a mechanism to encode it in the meta-info files, and propagate that data to the generation templates, without assigning specific meanings to the data points.

## Base terms

ZAP access control defines three base terms:

   1. **operation** : it's something that can be done. Example: read, write, invoke.
   2. **role**: it's a privilege of an actor. Such as "View privilege", "Administrative rol", etc.
   3. **modifiers**: special access control conditions, such as _fabric sensitive_ data or _fabric scoped_ data.

The base terms are defined in the metadata XML under a top tag `<accessControl>`. Example of access control base term definitions:

```
  <accessControl>
    <operation type="read" description="Read operation"/>
    <operation type="write" description="Write operation"/>
    <operation type="invoke" description="Invoke operation"/>
    <modifier type="fabric-scoped" description="Fabric-scoped data"/>
    <modifier type="fabric-sensitive" description="Fabric-sensitive data"/>
    <role type="view" description="View privilege"/>
    <role type="operate" description="Operate privilege"/>
    <role type="manage" description="Managing privilege"/>
    <role type="administer" description="Administrative privilege"/>
  </accessControl>

```

This example defines three operations, _read_, _write_ and _invoke_, two modifiers and four roles.

## Access triplets

Each individual access condition can be defined with an access triplet in the XML.
Access triplet is a combination of an _operation_, _role_ and _modifier_. They are optional, so you can only have one of these. A missing part of triplet generally means permissivenes, but that's implementation specific for the given SDK. An entity, that defines it's access can have one or more access triplets.

Example:
```
    <attribute side="server" code="0x0000" define="AT1" type="INT64U" writable="false" optional="true">
      <description>at1</description>
      <access op="write" role="manage" modifier="fabric-scoped"/>
    </attribute>
```
This is a definition of an attribute that has an access triplet, declaring it allows _write_ operation by a _manage_ role, with _fabric-scoped_ modifier applied.

## Default permissions

ZCL entities can define their own individual permissions. However, there is also a global definition of default permissions for given types. These are assumed for the given entity, unless is provides any specific permissions of its own.

Default permissions are declared via a `<defaultAccess>` tag at the toplevel of the XML.

Example:
```
  <defaultAccess type="command">
    <access op="invoke"/>
  </defaultAccess>
  <defaultAccess type="cluster">
    <access op="read"/>
    <access op="write"/>
  </defaultAccess>
  <defaultAccess type="attribute">
    <access op="read" role="view"/>
    <access op="write" role="operate"/>
  </defaultAccess>

```

## Template helpers

The basic template helper to use is the `{{#access}} ... {{/access}}` iterator. This iterator iterates over all the given access triplets.
It supports 2 options:
   * `entity="attribute/command/event"` - if the entity can't be determined from context, this sets the entity type.
   * `includeDefault="true/false"` - determines if default values are included or not.

Example:
```
{{#zcl_clusters}}
Cluster: {{name}} [{{code}}]
{{#zcl_attributes}}
  - attribute: {{name}} [{{code}}]
  {{#access entity="attribute"}}
    * Op: {{operation}} / Role: {{role}} / Modifier: {{accessModifier}}
  {{/access}}
{{/zcl_attributes}}
{{#zcl_commands}}
  - command: {{name}} [{{code}}]
  {{#access entity="command"}}
    * Op: {{operation}} / Role: {{role}} / Modifier: {{accessModifier}}
  {{/access}}
{{/zcl_commands}}
{{#zcl_events}}
  - event: {{name}} [{{code}}]
  {{#access entity="event"}}
    * Op: {{operation}} / Role: {{role}} / Modifier: {{accessModifier}}
  {{/access}}
{{/zcl_events}}

{{/zcl_clusters}}
```