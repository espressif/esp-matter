# The GN chip component provides libCHIP.a as the `matterlib` target, and apps such as
# examples/zap_light link it, some before this component is processed. Define it here,
# before any component; CMakeLists.txt links it to this component's library.
add_library(matterlib INTERFACE)
