// Copyright 2021 The Pigweed Authors
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy of
// the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations under
// the License.

package dev.pigweed.pw_rpc;

import com.google.common.collect.ImmutableMap;
import com.google.protobuf.MessageLite;
import java.util.Arrays;
import java.util.stream.Collectors;

/** Represents an RPC service: a collection of related methods. */
public class Service {
  private final String name;
  private final int id;
  private final ImmutableMap<Integer, Method> methods;

  public Service(String name, Method.Builder... methodBuilders) {
    this.name = name;
    this.id = Ids.calculate(name);
    Arrays.stream(methodBuilders).forEach(m -> m.setService(this));
    this.methods = ImmutableMap.copyOf(Arrays.stream(methodBuilders)
                                           .map(Method.Builder::build)
                                           .collect(Collectors.toMap(Method::id, m -> m)));
  }

  public String name() {
    return name;
  }

  int id() {
    return id;
  }

  public ImmutableMap<Integer, Method> methods() {
    return methods;
  }

  public final Method method(String name) {
    return methods().get(Ids.calculate(name));
  }

  @Override
  public final String toString() {
    return name();
  }

  public static Method.Builder unaryMethod(
      String name, Class<? extends MessageLite> request, Class<? extends MessageLite> response) {
    return Method.builder()
        .setType(Method.Type.UNARY)
        .setName(name)
        .setRequest(request)
        .setResponse(response);
  }

  public static Method.Builder serverStreamingMethod(
      String name, Class<? extends MessageLite> request, Class<? extends MessageLite> response) {
    return Method.builder()
        .setType(Method.Type.SERVER_STREAMING)
        .setName(name)
        .setRequest(request)
        .setResponse(response);
  }

  public static Method.Builder clientStreamingMethod(
      String name, Class<? extends MessageLite> request, Class<? extends MessageLite> response) {
    return Method.builder()
        .setType(Method.Type.CLIENT_STREAMING)
        .setName(name)
        .setRequest(request)
        .setResponse(response);
  }

  public static Method.Builder bidirectionalStreamingMethod(
      String name, Class<? extends MessageLite> request, Class<? extends MessageLite> response) {
    return Method.builder()
        .setType(Method.Type.BIDIRECTIONAL_STREAMING)
        .setName(name)
        .setRequest(request)
        .setResponse(response);
  }
}
