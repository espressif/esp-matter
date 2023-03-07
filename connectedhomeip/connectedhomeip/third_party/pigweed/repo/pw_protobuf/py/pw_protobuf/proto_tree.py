# Copyright 2020 The Pigweed Authors
#
# Licensed under the Apache License, Version 2.0 (the "License"); you may not
# use this file except in compliance with the License. You may obtain a copy of
# the License at
#
#     https://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
# WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
# License for the specific language governing permissions and limitations under
# the License.
"""This module defines data structures for protobuf entities."""

import abc
import collections
import enum
import itertools

from typing import (
    Callable,
    Dict,
    Iterator,
    List,
    Optional,
    Tuple,
    TypeVar,
    cast,
)

from google.protobuf import descriptor_pb2

from pw_protobuf import options, symbol_name_mapping
from pw_protobuf_codegen_protos.options_pb2 import Options

T = TypeVar('T')  # pylint: disable=invalid-name

# Currently, protoc does not do a traversal to look up the package name of all
# messages that are referenced in the file. For such "external" message names,
# we are unable to find where the "::pwpb" subnamespace would be inserted by our
# codegen. This namespace provides us with an alternative, more verbose
# namespace that the codegen can use as a fallback in these cases. For example,
# for the symbol name `my.external.package.ProtoMsg.SubMsg`, we would use
# `::pw::pwpb_codegen_private::my::external::package:ProtoMsg::SubMsg` to refer
# to the pw_protobuf generated code, when package name info is not available.
#
# TODO(b/258832150) Explore removing this if possible
EXTERNAL_SYMBOL_WORKAROUND_NAMESPACE = 'pw::pwpb_codegen_private'


class ProtoNode(abc.ABC):
    """A ProtoNode represents a C++ scope mapping of an entity in a .proto file.

    Nodes form a tree beginning at a top-level (global) scope, descending into a
    hierarchy of .proto packages and the messages and enums defined within them.
    """

    class Type(enum.Enum):
        """The type of a ProtoNode.

        PACKAGE maps to a C++ namespace.
        MESSAGE maps to a C++ "Encoder" class within its own namespace.
        ENUM maps to a C++ enum within its parent's namespace.
        EXTERNAL represents a node defined within a different compilation unit.
        SERVICE represents an RPC service definition.
        """

        PACKAGE = 1
        MESSAGE = 2
        ENUM = 3
        EXTERNAL = 4
        SERVICE = 5

    def __init__(self, name: str):
        self._name: str = name
        self._children: Dict[str, 'ProtoNode'] = collections.OrderedDict()
        self._parent: Optional['ProtoNode'] = None

    @abc.abstractmethod
    def type(self) -> 'ProtoNode.Type':
        """The type of the node."""

    def children(self) -> List['ProtoNode']:
        return list(self._children.values())

    def parent(self) -> Optional['ProtoNode']:
        return self._parent

    def name(self) -> str:
        return self._name

    def cpp_name(self) -> str:
        """The name of this node in generated C++ code."""
        return symbol_name_mapping.fix_cc_identifier(self._name).replace(
            '.', '::'
        )

    def _package_or_external(self) -> 'ProtoNode':
        """Returns this node's deepest package or external ancestor node.

        This method may need to return an external node, as a fallback for
        external names that are referenced, but not processed into a more
        regular proto tree. This is because there is no way to find the package
        name of a node referring to an external symbol.
        """
        node: Optional['ProtoNode'] = self
        while (
            node
            and node.type() != ProtoNode.Type.PACKAGE
            and node.type() != ProtoNode.Type.EXTERNAL
        ):
            node = node.parent()

        assert node, 'proto tree was built without a root'
        return node

    def cpp_namespace(
        self,
        root: Optional['ProtoNode'] = None,
        codegen_subnamespace: Optional[str] = 'pwpb',
    ) -> str:
        """C++ namespace of the node, up to the specified root.

        Args:
          root: Namespace from which this ProtoNode is referred. If this
            ProtoNode has `root` as an ancestor namespace, then the ancestor
            namespace scopes above `root` are omitted.

          codegen_subnamespace: A subnamespace that is appended to the package
            declared in the .proto file. It is appended to the declared package,
            but before any namespaces that are needed for messages etc. This
            feature can be used to allow different codegen tools to output
            different, non-conflicting symbols for the same protos.

            By default, this is "pwpb", which reflects the default behaviour
            of the pwpb codegen.
        """
        self_pkg_or_ext = self._package_or_external()
        root_pkg_or_ext = (
            root._package_or_external()  # pylint: disable=protected-access
            if root is not None
            else None
        )
        if root_pkg_or_ext:
            assert root_pkg_or_ext.type() != ProtoNode.Type.EXTERNAL

        def compute_hierarchy() -> Iterator[str]:
            same_package = True

            if self_pkg_or_ext.type() == ProtoNode.Type.EXTERNAL:
                # Can't figure out where the namespace cutoff is. Punt to using
                # the external symbol workaround.
                #
                # TODO(b/250945489) Investigate removing this limitation / hack
                return itertools.chain(
                    [EXTERNAL_SYMBOL_WORKAROUND_NAMESPACE],
                    self._attr_hierarchy(ProtoNode.cpp_name, root=None),
                )

            if root is None or root_pkg_or_ext is None:  # extra check for mypy
                # TODO(b/250945489): maybe elide "::{codegen_subnamespace}"
                # here, if this node doesn't have any package?
                same_package = False
            else:
                paired_hierarchy = itertools.zip_longest(
                    self_pkg_or_ext._attr_hierarchy(  # pylint: disable=protected-access
                        ProtoNode.cpp_name, root=None
                    ),
                    root_pkg_or_ext._attr_hierarchy(  # pylint: disable=protected-access
                        ProtoNode.cpp_name, root=None
                    ),
                )
                for str_a, str_b in paired_hierarchy:
                    if str_a != str_b:
                        same_package = False
                        break

            if same_package:
                # This ProtoNode and the requested root are in the same package,
                # so the `codegen_subnamespace` should be omitted.
                hierarchy = self._attr_hierarchy(ProtoNode.cpp_name, root)
                return hierarchy

            # The given root is either effectively nonexistent (common ancestor
            # is ""), or is only a partial match for the package of this node.
            # Either way, we will have to insert `codegen_subnamespace` after
            # the relevant package string.
            package_hierarchy = self_pkg_or_ext._attr_hierarchy(  # pylint: disable=protected-access
                ProtoNode.cpp_name, root
            )
            maybe_subnamespace = (
                [codegen_subnamespace] if codegen_subnamespace else []
            )
            inside_hierarchy = self._attr_hierarchy(
                ProtoNode.cpp_name, self_pkg_or_ext
            )

            hierarchy = itertools.chain(
                package_hierarchy, maybe_subnamespace, inside_hierarchy
            )
            return hierarchy

        joined_namespace = '::'.join(
            name for name in compute_hierarchy() if name
        )

        return (
            '' if joined_namespace == codegen_subnamespace else joined_namespace
        )

    def proto_path(self) -> str:
        """Fully-qualified package path of the node."""
        path = '.'.join(self._attr_hierarchy(lambda node: node.name(), None))
        return path.lstrip('.')

    def pwpb_struct(self) -> str:
        """Name of the pw_protobuf struct for this proto."""
        return '::' + self.cpp_namespace() + '::Message'

    def pwpb_table(self) -> str:
        """Name of the pw_protobuf table constant for this proto."""
        return '::' + self.cpp_namespace() + '::kMessageFields'

    def nanopb_fields(self) -> str:
        """Name of the Nanopb variable that represents the proto fields."""
        return self._nanopb_name() + '_fields'

    def nanopb_struct(self) -> str:
        """Name of the Nanopb struct for this proto."""
        return '::' + self._nanopb_name()

    def _nanopb_name(self) -> str:
        name = '_'.join(self._attr_hierarchy(lambda node: node.name(), None))
        return name.lstrip('_')

    def common_ancestor(self, other: 'ProtoNode') -> Optional['ProtoNode']:
        """Finds the earliest common ancestor of this node and other."""

        if other is None:
            return None

        own_depth = self.depth()
        other_depth = other.depth()
        diff = abs(own_depth - other_depth)

        if own_depth < other_depth:
            first: Optional['ProtoNode'] = self
            second: Optional['ProtoNode'] = other
        else:
            first = other
            second = self

        while diff > 0:
            assert second is not None
            second = second.parent()
            diff -= 1

        while first != second:
            if first is None or second is None:
                return None

            first = first.parent()
            second = second.parent()

        return first

    def depth(self) -> int:
        """Returns the depth of this node from the root."""
        depth = 0
        node = self._parent
        while node:
            depth += 1
            node = node.parent()
        return depth

    def add_child(self, child: 'ProtoNode') -> None:
        """Inserts a new node into the tree as a child of this node.

        Args:
          child: The node to insert.

        Raises:
          ValueError: This node does not allow nesting the given type of child.
        """
        if not self._supports_child(child):
            raise ValueError(
                'Invalid child %s for node of type %s'
                % (child.type(), self.type())
            )

        # pylint: disable=protected-access
        if child._parent is not None:
            del child._parent._children[child.name()]

        child._parent = self
        self._children[child.name()] = child
        # pylint: enable=protected-access

    def find(self, path: str) -> Optional['ProtoNode']:
        """Finds a node within this node's subtree.

        Args:
          path: The path to the sought node.
        """
        node = self

        # pylint: disable=protected-access
        for section in path.split('.'):
            child = node._children.get(section)
            if child is None:
                return None
            node = child
        # pylint: enable=protected-access

        return node

    def __iter__(self) -> Iterator['ProtoNode']:
        """Iterates depth-first through all nodes in this node's subtree."""
        yield self
        for child_iterator in self._children.values():
            for child in child_iterator:
                yield child

    def _attr_hierarchy(
        self,
        attr_accessor: Callable[['ProtoNode'], T],
        root: Optional['ProtoNode'],
    ) -> Iterator[T]:
        """Fetches node attributes at each level of the tree from the root.

        Args:
          attr_accessor: Function which extracts attributes from a ProtoNode.
          root: The node at which to terminate.

        Returns:
          An iterator to a list of the selected attributes from the root to the
          current node.
        """
        hierarchy = []
        node: Optional['ProtoNode'] = self
        while node is not None and node != root:
            hierarchy.append(attr_accessor(node))
            node = node.parent()
        return reversed(hierarchy)

    @abc.abstractmethod
    def _supports_child(self, child: 'ProtoNode') -> bool:
        """Returns True if child is a valid child type for the current node."""


class ProtoPackage(ProtoNode):
    """A protobuf package."""

    def type(self) -> ProtoNode.Type:
        return ProtoNode.Type.PACKAGE

    def _supports_child(self, child: ProtoNode) -> bool:
        return True


class ProtoEnum(ProtoNode):
    """Representation of an enum in a .proto file."""

    def __init__(self, name: str):
        super().__init__(name)
        self._values: List[Tuple[str, int]] = []

    def type(self) -> ProtoNode.Type:
        return ProtoNode.Type.ENUM

    def values(self) -> List[Tuple[str, int]]:
        return list(self._values)

    def add_value(self, name: str, value: int) -> None:
        self._values.append(
            (
                ProtoMessageField.upper_snake_case(
                    symbol_name_mapping.fix_cc_enum_value_name(name)
                ),
                value,
            )
        )

    def _supports_child(self, child: ProtoNode) -> bool:
        # Enums cannot have nested children.
        return False


class ProtoMessage(ProtoNode):
    """Representation of a message in a .proto file."""

    def __init__(self, name: str):
        super().__init__(name)
        self._fields: List['ProtoMessageField'] = []
        self._dependencies: Optional[List['ProtoMessage']] = None
        self._dependency_cycles: List['ProtoMessage'] = []

    def type(self) -> ProtoNode.Type:
        return ProtoNode.Type.MESSAGE

    def fields(self) -> List['ProtoMessageField']:
        return list(self._fields)

    def add_field(self, field: 'ProtoMessageField') -> None:
        self._fields.append(field)

    def _supports_child(self, child: ProtoNode) -> bool:
        return (
            child.type() == self.Type.ENUM or child.type() == self.Type.MESSAGE
        )

    def dependencies(self) -> List['ProtoMessage']:
        if self._dependencies is None:
            self._dependencies = []
            for field in self._fields:
                if (
                    field.type()
                    != descriptor_pb2.FieldDescriptorProto.TYPE_MESSAGE
                ):
                    continue

                type_node = field.type_node()
                assert type_node is not None
                if type_node.type() == ProtoNode.Type.MESSAGE:
                    self._dependencies.append(cast(ProtoMessage, type_node))

        return list(self._dependencies)

    def dependency_cycles(self) -> List['ProtoMessage']:
        return list(self._dependency_cycles)

    def remove_dependency_cycle(self, dependency: 'ProtoMessage'):
        assert self._dependencies is not None
        assert dependency in self._dependencies
        self._dependencies.remove(dependency)
        self._dependency_cycles.append(dependency)


class ProtoService(ProtoNode):
    """Representation of a service in a .proto file."""

    def __init__(self, name: str):
        super().__init__(name)
        self._methods: List['ProtoServiceMethod'] = []

    def type(self) -> ProtoNode.Type:
        return ProtoNode.Type.SERVICE

    def methods(self) -> List['ProtoServiceMethod']:
        return list(self._methods)

    def add_method(self, method: 'ProtoServiceMethod') -> None:
        self._methods.append(method)

    def _supports_child(self, child: ProtoNode) -> bool:
        return False


class ProtoExternal(ProtoNode):
    """A node from a different compilation unit.

    An external node is one that isn't defined within the current compilation
    unit, most likely as it comes from an imported proto file. Its type is not
    known, so it does not have any members or additional data. Its purpose
    within the node graph is to provide namespace resolution between compile
    units.
    """

    def type(self) -> ProtoNode.Type:
        return ProtoNode.Type.EXTERNAL

    def _supports_child(self, child: ProtoNode) -> bool:
        return True


# This class is not a node and does not appear in the proto tree.
# Fields belong to proto messages and are processed separately.
class ProtoMessageField:
    """Representation of a field within a protobuf message."""

    def __init__(
        self,
        field_name: str,
        field_number: int,
        field_type: int,
        type_node: Optional[ProtoNode] = None,
        optional: bool = False,
        repeated: bool = False,
        field_options: Optional[Options] = None,
    ):
        self._field_name = symbol_name_mapping.fix_cc_identifier(field_name)
        self._number: int = field_number
        self._type: int = field_type
        self._type_node: Optional[ProtoNode] = type_node
        self._optional: bool = optional
        self._repeated: bool = repeated
        self._options: Optional[Options] = field_options

    def name(self) -> str:
        return self.upper_camel_case(self._field_name)

    def field_name(self) -> str:
        return self._field_name

    def enum_name(self) -> str:
        return self.upper_snake_case(
            symbol_name_mapping.fix_cc_enum_value_name(self._field_name)
        )

    def number(self) -> int:
        return self._number

    def type(self) -> int:
        return self._type

    def type_node(self) -> Optional[ProtoNode]:
        return self._type_node

    def is_optional(self) -> bool:
        return self._optional

    def is_repeated(self) -> bool:
        return self._repeated

    def options(self) -> Optional[Options]:
        return self._options

    @staticmethod
    def upper_camel_case(field_name: str) -> str:
        """Converts a field name to UpperCamelCase."""
        name_components = field_name.split('_')
        return ''.join([word.lower().capitalize() for word in name_components])

    @staticmethod
    def upper_snake_case(field_name: str) -> str:
        """Converts a field name to UPPER_SNAKE_CASE."""
        return field_name.upper()


class ProtoServiceMethod:
    """A method defined in a protobuf service."""

    class Type(enum.Enum):
        UNARY = 'kUnary'
        SERVER_STREAMING = 'kServerStreaming'
        CLIENT_STREAMING = 'kClientStreaming'
        BIDIRECTIONAL_STREAMING = 'kBidirectionalStreaming'

        def cc_enum(self) -> str:
            """Returns the pw_rpc MethodType C++ enum for this method type."""
            return '::pw::rpc::MethodType::' + self.value

    def __init__(
        self,
        service: ProtoService,
        name: str,
        method_type: Type,
        request_type: ProtoNode,
        response_type: ProtoNode,
    ):
        self._service = service
        self._name = name
        self._type = method_type
        self._request_type = request_type
        self._response_type = response_type

    def service(self) -> ProtoService:
        return self._service

    def name(self) -> str:
        return self._name

    def type(self) -> Type:
        return self._type

    def server_streaming(self) -> bool:
        return self._type in (
            self.Type.SERVER_STREAMING,
            self.Type.BIDIRECTIONAL_STREAMING,
        )

    def client_streaming(self) -> bool:
        return self._type in (
            self.Type.CLIENT_STREAMING,
            self.Type.BIDIRECTIONAL_STREAMING,
        )

    def request_type(self) -> ProtoNode:
        return self._request_type

    def response_type(self) -> ProtoNode:
        return self._response_type


def _add_enum_fields(enum_node: ProtoNode, proto_enum) -> None:
    """Adds fields from a protobuf enum descriptor to an enum node."""
    assert enum_node.type() == ProtoNode.Type.ENUM
    enum_node = cast(ProtoEnum, enum_node)

    for value in proto_enum.value:
        enum_node.add_value(value.name, value.number)


def _create_external_nodes(root: ProtoNode, path: str) -> ProtoNode:
    """Creates external nodes for a path starting from the given root."""

    node = root
    for part in path.split('.'):
        child = node.find(part)
        if not child:
            child = ProtoExternal(part)
            node.add_child(child)
        node = child

    return node


def _find_or_create_node(
    global_root: ProtoNode, package_root: ProtoNode, path: str
) -> ProtoNode:
    """Searches the proto tree for a node by path, creating it if not found."""

    if path[0] == '.':
        # Fully qualified path.
        root_relative_path = path[1:]
        search_root = global_root
    else:
        root_relative_path = path
        search_root = package_root

    node = search_root.find(root_relative_path)
    if node is None:
        # Create nodes for field types that don't exist within this
        # compilation context, such as those imported from other .proto
        # files.
        node = _create_external_nodes(search_root, root_relative_path)

    return node


def _add_message_fields(
    global_root: ProtoNode,
    package_root: ProtoNode,
    message: ProtoNode,
    proto_message,
    proto_options,
) -> None:
    """Adds fields from a protobuf message descriptor to a message node."""
    assert message.type() == ProtoNode.Type.MESSAGE
    message = cast(ProtoMessage, message)

    type_node: Optional[ProtoNode]

    for field in proto_message.field:
        if field.type_name:
            # The "type_name" member contains the global .proto path of the
            # field's type object, for example ".pw.protobuf.test.KeyValuePair".
            # Try to find the node for this object within the current context.
            type_node = _find_or_create_node(
                global_root, package_root, field.type_name
            )
        else:
            type_node = None

        optional = field.proto3_optional
        repeated = (
            field.label == descriptor_pb2.FieldDescriptorProto.LABEL_REPEATED
        )
        field_options = (
            options.match_options(
                '.'.join((message.proto_path(), field.name)), proto_options
            )
            if proto_options is not None
            else None
        )
        message.add_field(
            ProtoMessageField(
                field.name,
                field.number,
                field.type,
                type_node,
                optional,
                repeated,
                field_options,
            )
        )


def _add_service_methods(
    global_root: ProtoNode,
    package_root: ProtoNode,
    service: ProtoNode,
    proto_service,
) -> None:
    assert service.type() == ProtoNode.Type.SERVICE
    service = cast(ProtoService, service)

    for method in proto_service.method:
        if method.client_streaming and method.server_streaming:
            method_type = ProtoServiceMethod.Type.BIDIRECTIONAL_STREAMING
        elif method.client_streaming:
            method_type = ProtoServiceMethod.Type.CLIENT_STREAMING
        elif method.server_streaming:
            method_type = ProtoServiceMethod.Type.SERVER_STREAMING
        else:
            method_type = ProtoServiceMethod.Type.UNARY

        request_node = _find_or_create_node(
            global_root, package_root, method.input_type
        )
        response_node = _find_or_create_node(
            global_root, package_root, method.output_type
        )

        service.add_method(
            ProtoServiceMethod(
                service, method.name, method_type, request_node, response_node
            )
        )


def _populate_fields(
    proto_file: descriptor_pb2.FileDescriptorProto,
    global_root: ProtoNode,
    package_root: ProtoNode,
    proto_options: Optional[options.FieldOptions],
) -> None:
    """Traverses a proto file, adding all message and enum fields to a tree."""

    def populate_message(node, message):
        """Recursively populates nested messages and enums."""
        _add_message_fields(
            global_root, package_root, node, message, proto_options
        )

        for proto_enum in message.enum_type:
            _add_enum_fields(node.find(proto_enum.name), proto_enum)
        for msg in message.nested_type:
            populate_message(node.find(msg.name), msg)

    # Iterate through the proto file, populating top-level objects.
    for proto_enum in proto_file.enum_type:
        enum_node = package_root.find(proto_enum.name)
        assert enum_node is not None
        _add_enum_fields(enum_node, proto_enum)

    for message in proto_file.message_type:
        populate_message(package_root.find(message.name), message)

    for service in proto_file.service:
        service_node = package_root.find(service.name)
        assert service_node is not None
        _add_service_methods(global_root, package_root, service_node, service)


def _build_hierarchy(
    proto_file: descriptor_pb2.FileDescriptorProto,
) -> Tuple[ProtoPackage, ProtoPackage]:
    """Creates a ProtoNode hierarchy from a proto file descriptor."""

    root = ProtoPackage('')
    package_root = root

    for part in proto_file.package.split('.'):
        package = ProtoPackage(part)
        package_root.add_child(package)
        package_root = package

    def build_message_subtree(proto_message):
        node = ProtoMessage(proto_message.name)
        for proto_enum in proto_message.enum_type:
            node.add_child(ProtoEnum(proto_enum.name))
        for submessage in proto_message.nested_type:
            node.add_child(build_message_subtree(submessage))

        return node

    for proto_enum in proto_file.enum_type:
        package_root.add_child(ProtoEnum(proto_enum.name))

    for message in proto_file.message_type:
        package_root.add_child(build_message_subtree(message))

    for service in proto_file.service:
        package_root.add_child(ProtoService(service.name))

    return root, package_root


def build_node_tree(
    file_descriptor_proto: descriptor_pb2.FileDescriptorProto,
    proto_options: Optional[options.FieldOptions] = None,
) -> Tuple[ProtoNode, ProtoNode]:
    """Constructs a tree of proto nodes from a file descriptor.

    Returns the root node of the entire proto package tree and the node
    representing the file's package.
    """
    global_root, package_root = _build_hierarchy(file_descriptor_proto)
    _populate_fields(
        file_descriptor_proto, global_root, package_root, proto_options
    )
    return global_root, package_root
